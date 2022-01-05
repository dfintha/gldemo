#include "gl.hpp"
#include "engine/scene.hpp"
#include "geometry/vector.hpp"
#include "objects/objects.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>

eng::scene scene;

constexpr const char title[] = "gldemo";
constexpr const char description[] =
    "Click inside the window to toggle mouse grab. When the mouse is grabbed,\n"
    "you can use it to look around freely. Move forward using the W key, and\n"
    "backwards using the S key. You can toggle full screen mode using the F\n"
    "key. Holding Shift toggles slow movement.";

void e_init() {
	glutSetWindowTitle(title);
	glViewport(0, 0, scene.width, scene.height);
    srand(unsigned(time(nullptr)));

    scene.cam.position = geo::vector(0, 1, -3);
    scene.cam.lookat = geo::vector(0, 1, -2);
    scene.cam.up = geo::vector(0, 1, 0);
    scene.ambient_color = geo::vector(0.2, 0.2, 0.2, 1);
    scene.light_color = geo::vector(1, 1, 1, 1);
    scene.light_position = geo::vector(0, 20, -10, 1);
    scene.background_color = geo::vector(0.1, 0.1, 0.1, 1);

    const eng::texture_ptr checker =
        tex::checkerboard<16>(geo::vector(0.2, 0.2, 0.8, 1),
                              geo::vector(0.1, 0.1, 0.2, 1));

    const eng::texture_ptr hstripe =
        tex::hstripes<32>(geo::vector(0.2, 0.8, 0.2, 1),
                          geo::vector(0.1, 0.3, 0.1, 1));

    const eng::texture_ptr vstripe =
        tex::vstripes<32>(geo::vector(0.8, 0.2, 0.2, 1),
                         geo::vector(0.3, 0.1, 0.1, 1));

    const eng::texture_ptr silvershine =
        tex::randomized<32>(geo::vector(0.60, 0.60, 0.60, 1),
                            geo::vector(0.70, 0.70, 0.70, 1));

    auto current = std::shared_ptr<obj::object_base>(new obj::sphere);
    current->position = geo::vector(0, 1, -3);
    current->scale = geo::vector(25, 25, 25);
    current->ambient = geo::vector(0.1, 0.1, 0.1, 1);
    current->diffuse = geo::vector(0.2, 0.2, 0.2, 1);
    current->specular = geo::vector(1, 1, 1, 1);
    current->shine = 40;
    current->draw_side = obj::inside;
    current->draw_mode = obj::polywire;
    current->create();
    scene.objects.push_back(current);

    current = std::shared_ptr<obj::object_base>(new obj::sphere);
    current->position = scene.light_position;
    current->scale = geo::vector(0.5, 0.5, 0.5, 1);
    current->ambient = geo::vector(1, 1, 1, 1);
    current->diffuse = geo::vector(1, 1, 1, 1);
    current->specular = geo::vector(1, 1, 1, 1);
    current->shine = 1;
    current->draw_mode = obj::polywire;
    current->create();
    scene.objects.push_back(current);

    current = std::shared_ptr<obj::object_base>(new obj::cylinder);
    current->position = geo::vector(-9, 0, 4);
    current->scale = geo::vector(1, 10, 1, 1);
    current->ambient = geo::vector(1, 1, 1, 1);
    current->diffuse = geo::vector(0.2, 0.6, 0.8, 1);
    current->specular = geo::vector();
    current->shine = 15;
    current->textured = true;
    current->texture = hstripe;
    current->create();
    scene.objects.push_back(current);

    constexpr const char modelpath[] = "model/bunny.obj";
    current = std::shared_ptr<obj::object_base>(new obj::obj_model(modelpath));
    current->position = geo::vector(9, 0, 4);
    current->scale = geo::vector(4, 4, 4, 1);
    current->rotation = geo::vector(0, 1, 0);
    current->angle = 3.1415;
    current->ambient = geo::vector(1, 1, 1);
    current->diffuse = geo::vector(0.6, 0.6, 0.6);
    current->specular = geo::vector(0.8, 0.8, 0.8);
    current->shine = 32;
    current->draw_mode = obj::polyfill;
    current->textured = true;
    current->texture = silvershine;
    current->create();
    scene.objects.push_back(current);

    current = std::shared_ptr<obj::object_base>(new obj::rectangle);
    current->position = geo::vector(0, 0, 0, 1);
    current->scale = geo::vector(100, 10, 100);
    current->ambient = geo::vector(0.3, 0.3, 0.3, 1);
    current->diffuse = geo::vector(0.2, 0.2, 1, 1);
    current->specular = geo::vector(0.2, 0.2, 0.2, 1);
    current->shine = 40;
    current->textured = true;
    current->texture = vstripe;
    current->create();
    scene.objects.push_back(current);

    constexpr const int amount = 8;
    for (int i = 0; i <= amount - 1; ++i) {
    	current = std::shared_ptr<obj::object_base>(new obj::sphere);
        const float density = float(i) / amount;
        current->position = geo::vector(1 - amount + 2 * i, 1.5, 10);
        current->ambient = geo::vector(0.1, 0.1, 0.1, 1);
        current->diffuse = geo::vector(0.1, 0.1, 0.2, 1);
        current->specular = geo::vector(density, density, density, 1);
        current->shine = 40;
        current->textured = true;
        current->texture = checker;
        current->create();
        scene.objects.push_back(current);
    }

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    scene.program.vertex.make("shader/phong.vert", GL_VERTEX_SHADER);
    scene.program.fragment.make("shader/phong.frag", GL_FRAGMENT_SHADER);

    glBindAttribLocation(scene.program, 0, "position");
    glBindAttribLocation(scene.program, 1, "normal");
    glBindAttribLocation(scene.program, 2, "texture");
    glBindFragDataLocation(scene.program, 0, "fragmentColor");

    scene.program.make();
    scene.program.use();

    std::cout << description << std::endl;
}

void e_display() {
	const geo::vector& bg = scene.background_color;
    glClearColor(bg.x, bg.y, bg.z, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& object : scene.objects) {
        object->draw(scene);
    }

    glutSwapBuffers();
}

void e_mouse(int button, int state, int, int) {
	const float cx = scene.width / 2;
    const float cy = scene.height / 2;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        scene.grab_mouse = !scene.grab_mouse;
        glutSetCursor(scene.grab_mouse ? GLUT_CURSOR_NONE : GLUT_CURSOR_INHERIT);
        glutWarpPointer(cx, cy);
    }
}

void e_idle() {
	glutPostRedisplay();
}

void e_keyboard(unsigned char key, int, int) {
	switch (key) {
        case 'q':
        case 'Q':
            glutLeaveMainLoop();
            break;

        case 'f':
        case 'F':
            scene.toggle_full_screen();
            break;

        case 'w':
            scene.cam.step(0.2F, true);
            __attribute__((fallthrough));
        case 'W':
            scene.cam.step(0.2F, true);
            break;

        case 's':
            scene.cam.step(0.2F, false);
            __attribute__((fallthrough));
        case 'S':
            scene.cam.step(0.2F, false);
            break;

        case 'a':
        case 'A':
            break;

        case 'd':
        case 'D':
            break;

        case 'r':
        case 'R':
            scene.cam.position = geo::vector(0, 1, -3);
            scene.cam.lookat = geo::vector(0, 1, -2);
            break;

        default:
	        break;
    }
}

void e_keyboard_up(unsigned char, int, int) { }

void e_special(int, int, int) { }

void e_motion(int, int) { }

void e_passive_motion(int x, int y) {
	const int cx = scene.width / 2;
    const int cy = scene.height / 2;

    if (scene.grab_mouse && (cx != x || cy != y)) {
        if (cx != x)
            scene.cam.xturn(float(cx - x) / 500);

        if (cy != y)
            scene.cam.yturn(float(cy - y) / 500);

        glutWarpPointer(cx, cy);
    }
}

void e_reshape(int width, int height) {
	scene.cam.aspect_ratio = float(width) / height;
    scene.width = width;
    scene.height = height;
    glViewport(0, 0, width, height);
}

void e_exit() {
	scene.program.dispose();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
#if !defined(__APPLE__)
    glutInitContextVersion(3, 3);
#endif
    glutInitWindowSize(scene.width, scene.height);
    glutInitWindowPosition(100, 100);
#if defined(__APPLE__)
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH |
                        GLUT_3_3_CORE_PROFILE | GLUT_MULTISAMPLE);
#else
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH |
                        GLUT_MULTISAMPLE);
#endif
    glutCreateWindow(argv[0]);

#if !defined(__APPLE__)
    glewExperimental = true;
    glewInit();
#endif

    e_init();

    glutDisplayFunc(e_display);
    glutMouseFunc(e_mouse);
    glutIdleFunc(e_idle);
    glutKeyboardFunc(e_keyboard);
    glutKeyboardUpFunc(e_keyboard_up);
    glutSpecialFunc(e_special);
    glutMotionFunc(e_motion);
    glutPassiveMotionFunc(e_passive_motion);
    glutReshapeFunc(e_reshape);

    glutMainLoop();
    e_exit();

    return 0;
}
