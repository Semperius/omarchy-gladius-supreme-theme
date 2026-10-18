#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <gtk4-layer-shell.h>
#include <cairo.h>

#define MAX_PARTICLES 1200

typedef struct {
    double x, y;
    double vx, vy;
    double r0, r_max;
    double age, lifetime;
    double alpha0;
    double r_col, g_col, b_col;
    gboolean active;
} SmokeParticle;

typedef struct {
    GtkWidget *window;
    GtkWidget *drawing_area;
    GdkMonitor *monitor;
    int mon_x, mon_y, mon_w, mon_h;
} MonitorOverlay;

static SmokeParticle particles[MAX_PARTICLES];
static int particle_head = 0;
static pthread_mutex_t p_mutex = PTHREAD_MUTEX_INITIALIZER;

static volatile gboolean is_running = TRUE;
static volatile gboolean is_animating = FALSE;
static volatile gboolean needs_wake = FALSE;

static GList *overlay_list = NULL;
static GtkApplication *global_app = NULL;
static guint tick_source_id = 0;

static double rand_range(double min, double max) {
    return min + (double)rand() / (double)RAND_MAX * (max - min);
}

static void spawn_particle(double x, double y, double move_vx, double move_vy) {
    SmokeParticle *p = &particles[particle_head];
    particle_head = (particle_head + 1) % MAX_PARTICLES;

    double angle = rand_range(0, 2 * M_PI);
    double dist = rand_range(0, 7);
    p->x = x + cos(angle) * dist;
    p->y = y + sin(angle) * dist;

    // Opposite drift, slight upward rise, and mild atmospheric swirl
    p->vx = -move_vx * 0.12 + rand_range(-15, 15);
    p->vy = -move_vy * 0.12 - rand_range(12, 32); // thermal smoke ascent

    p->r0 = rand_range(12, 18);
    p->r_max = rand_range(42, 65);
    p->age = 0.0;
    p->lifetime = rand_range(0.70, 1.05);
    p->alpha0 = rand_range(0.40, 0.58);

    // Warhammer grimdark atmospheric smoke:
    // Volumetric cool misty smoke with subtle battle ash / ceramite mist hue
    double base = rand_range(0.82, 0.95);
    p->r_col = base * 0.94;
    p->g_col = base * 0.97;
    p->b_col = base * 1.04;
    if (p->b_col > 1.0) p->b_col = 1.0;

    p->active = TRUE;
}

static void on_map(GtkWidget *widget, gpointer data) {
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(widget));
    if (surface) {
        cairo_region_t *empty = cairo_region_create();
        gdk_surface_set_input_region(surface, empty);
        cairo_region_destroy(empty);
    }
}

static void draw_smoke_func(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
    MonitorOverlay *ov = (MonitorOverlay*)user_data;

    // Clear frame transparent
    cairo_save(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_restore(cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

    pthread_mutex_lock(&p_mutex);
    for (int i = 0; i < MAX_PARTICLES; i++) {
        SmokeParticle *p = &particles[i];
        if (!p->active) continue;

        double local_x = p->x - ov->mon_x;
        double local_y = p->y - ov->mon_y;

        double progress = p->age / p->lifetime;
        if (progress >= 1.0) continue;

        double radius = p->r0 + (p->r_max - p->r0) * pow(progress, 0.65);
        double alpha = p->alpha0 * pow(1.0 - progress, 1.4);

        if (local_x + radius < 0 || local_x - radius > width ||
            local_y + radius < 0 || local_y - radius > height) {
            continue;
        }

        // Volumetric radial smoke gradient
        cairo_pattern_t *pat = cairo_pattern_create_radial(
            local_x - radius * 0.15, local_y - radius * 0.15, radius * 0.05,
            local_x, local_y, radius
        );
        cairo_pattern_add_color_stop_rgba(pat, 0.0, p->r_col, p->g_col, p->b_col, alpha);
        cairo_pattern_add_color_stop_rgba(pat, 0.38, p->r_col * 0.92, p->g_col * 0.94, p->b_col * 0.98, alpha * 0.68);
        cairo_pattern_add_color_stop_rgba(pat, 0.72, p->r_col * 0.82, p->g_col * 0.84, p->b_col * 0.88, alpha * 0.22);
        cairo_pattern_add_color_stop_rgba(pat, 1.0, p->r_col * 0.70, p->g_col * 0.72, p->b_col * 0.78, 0.0);

        cairo_set_source(cr, pat);
        cairo_arc(cr, local_x, local_y, radius, 0, 2 * M_PI);
        cairo_fill(cr);
        cairo_pattern_destroy(pat);
    }
    pthread_mutex_unlock(&p_mutex);
}

static gint64 last_frame_time = 0;
static gboolean on_frame_tick(gpointer user_data);

static gboolean wake_animation(gpointer user_data) {
    if (!is_animating) {
        is_animating = TRUE;
        last_frame_time = g_get_monotonic_time();
        if (tick_source_id == 0) {
            tick_source_id = g_timeout_add(8, on_frame_tick, NULL);
        }
    }
    return G_SOURCE_REMOVE;
}

static gboolean on_frame_tick(gpointer user_data) {
    gint64 now = g_get_monotonic_time();
    if (last_frame_time == 0) last_frame_time = now;
    double dt = (double)(now - last_frame_time) / 1000000.0;
    last_frame_time = now;
    if (dt > 0.05) dt = 0.05;

    pthread_mutex_lock(&p_mutex);
    int active = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        SmokeParticle *p = &particles[i];
        if (!p->active) continue;

        p->age += dt;
        if (p->age >= p->lifetime) {
            p->active = FALSE;
            continue;
        }

        p->x += p->vx * dt;
        p->y += p->vy * dt;
        p->vx *= (1.0 - 0.35 * dt);
        p->vy *= (1.0 - 0.18 * dt);
        active++;
    }

    if (active == 0 && !needs_wake) {
        // All smoke cleared and no pending motion: sleep timer
        is_animating = FALSE;
        tick_source_id = 0;
        pthread_mutex_unlock(&p_mutex);

        // Final clear on all overlays
        for (GList *l = overlay_list; l != NULL; l = l->next) {
            MonitorOverlay *ov = (MonitorOverlay*)l->data;
            gtk_widget_queue_draw(ov->drawing_area);
        }
        return G_SOURCE_REMOVE;
    }
    needs_wake = FALSE;
    pthread_mutex_unlock(&p_mutex);

    // Queue redraw
    for (GList *l = overlay_list; l != NULL; l = l->next) {
        MonitorOverlay *ov = (MonitorOverlay*)l->data;
        gtk_widget_queue_draw(ov->drawing_area);
    }

    return G_SOURCE_CONTINUE;
}

// Background poller thread for Hyprland cursorpos
static void *poll_thread_func(void *arg) {
    const char *runtime = getenv("XDG_RUNTIME_DIR");
    const char *sig = getenv("HYPRLAND_INSTANCE_SIGNATURE");
    if (!runtime || !sig) {
        fprintf(stderr, "gladius-smoke-trail: Missing Hyprland session variables\n");
        return NULL;
    }

    char sock_path[512];
    snprintf(sock_path, sizeof(sock_path), "%s/hypr/%s/.socket.sock", runtime, sig);

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", sock_path);

    int prev_x = -1, prev_y = -1;

    while (is_running) {
        int fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fd >= 0) {
            if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
                const char *cmd = "cursorpos";
                write(fd, cmd, strlen(cmd));
                char buf[64];
                ssize_t n = read(fd, buf, sizeof(buf) - 1);
                if (n > 0) {
                    buf[n] = '\0';
                    int cx, cy;
                    if (sscanf(buf, "%d, %d", &cx, &cy) == 2) {
                        if (prev_x != -1 && (cx != prev_x || cy != prev_y)) {
                            double dx = cx - prev_x;
                            double dy = cy - prev_y;
                            double dist = sqrt(dx * dx + dy * dy);

                            if (dist > 1.5) {
                                pthread_mutex_lock(&p_mutex);
                                int steps = (int)(dist / 6.0);
                                if (steps < 1) steps = 1;
                                if (steps > 8) steps = 8;

                                for (int s = 0; s < steps; s++) {
                                    double f = (double)(s + 1) / (double)steps;
                                    double px = prev_x + dx * f;
                                    double py = prev_y + dy * f;
                                    spawn_particle(px, py, dx * 8.0, dy * 8.0);
                                }
                                needs_wake = TRUE;
                                pthread_mutex_unlock(&p_mutex);

                                if (!is_animating) {
                                    g_idle_add(wake_animation, NULL);
                                }
                            }
                        }
                        prev_x = cx;
                        prev_y = cy;
                    }
                }
            }
            close(fd);
        }
        usleep(8000); // 125 Hz
    }
    return NULL;
}

static void activate(GtkApplication *app, gpointer user_data) {
    global_app = app;
    GdkDisplay *display = gdk_display_get_default();

    GtkCssProvider *css = gtk_css_provider_new();
    gtk_css_provider_load_from_string(css, "window, drawingarea { background-color: transparent; }");
    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css);

    GListModel *monitors = gdk_display_get_monitors(display);
    guint n = g_list_model_get_n_items(monitors);

    for (guint i = 0; i < n; i++) {
        GdkMonitor *mon = GDK_MONITOR(g_list_model_get_item(monitors, i));
        GdkRectangle geom;
        gdk_monitor_get_geometry(mon, &geom);

        MonitorOverlay *ov = g_new0(MonitorOverlay, 1);
        ov->monitor = mon;
        ov->mon_x = geom.x;
        ov->mon_y = geom.y;
        ov->mon_w = geom.width;
        ov->mon_h = geom.height;

        ov->window = gtk_application_window_new(app);
        gtk_layer_init_for_window(GTK_WINDOW(ov->window));
        gtk_layer_set_monitor(GTK_WINDOW(ov->window), mon);
        gtk_layer_set_layer(GTK_WINDOW(ov->window), GTK_LAYER_SHELL_LAYER_OVERLAY);
        gtk_layer_set_namespace(GTK_WINDOW(ov->window), "gladius-smoke-trail");
        gtk_layer_set_keyboard_mode(GTK_WINDOW(ov->window), GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);
        gtk_layer_set_anchor(GTK_WINDOW(ov->window), GTK_LAYER_SHELL_EDGE_TOP, TRUE);
        gtk_layer_set_anchor(GTK_WINDOW(ov->window), GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);
        gtk_layer_set_anchor(GTK_WINDOW(ov->window), GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
        gtk_layer_set_anchor(GTK_WINDOW(ov->window), GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);
        gtk_layer_set_exclusive_zone(GTK_WINDOW(ov->window), -1);

        gtk_widget_set_can_target(ov->window, FALSE);
        gtk_widget_set_can_focus(ov->window, FALSE);

        ov->drawing_area = gtk_drawing_area_new();
        gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(ov->drawing_area), draw_smoke_func, ov, NULL);
        gtk_widget_set_can_target(ov->drawing_area, FALSE);
        gtk_widget_set_can_focus(ov->drawing_area, FALSE);

        gtk_window_set_child(GTK_WINDOW(ov->window), ov->drawing_area);
        g_signal_connect(ov->window, "map", G_CALLBACK(on_map), NULL);

        gtk_window_present(GTK_WINDOW(ov->window));
        overlay_list = g_list_append(overlay_list, ov);
        g_object_unref(mon);
    }

    // Start background poller thread
    pthread_t tid;
    pthread_create(&tid, NULL, poll_thread_func, NULL);
    pthread_detach(tid);
}

static void handle_signal(int sig) {
    is_running = FALSE;
    if (global_app) {
        g_application_quit(G_APPLICATION(global_app));
    }
}

int main(int argc, char **argv) {
    // Single instance lock
    const char *runtime = getenv("XDG_RUNTIME_DIR");
    char lock_path[512];
    snprintf(lock_path, sizeof(lock_path), "%s/gladius-smoke-trail.lock", runtime ? runtime : "/tmp");
    int lock_fd = open(lock_path, O_CREAT | O_RDWR, 0600);
    if (lock_fd >= 0 && flock(lock_fd, LOCK_EX | LOCK_NB) != 0) {
        // If already running and user called with --kill / -k:
        if (argc > 1 && (strcmp(argv[1], "--kill") == 0 || strcmp(argv[1], "-k") == 0)) {
            char pid_buf[32];
            int read_n = read(lock_fd, pid_buf, sizeof(pid_buf) - 1);
            if (read_n > 0) {
                pid_buf[read_n] = '\0';
                pid_t running_pid = atoi(pid_buf);
                if (running_pid > 0) kill(running_pid, SIGTERM);
            }
            return 0;
        }
        printf("gladius-smoke-trail is already running.\n");
        return 0;
    }

    if (lock_fd >= 0) {
        ftruncate(lock_fd, 0);
        dprintf(lock_fd, "%d\n", getpid());
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    srand(time(NULL));
    GtkApplication *app = gtk_application_new("org.omarchy.gladius.smoketrail", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
