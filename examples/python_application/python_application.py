from mx_engine import *
import os

def on_create():
    mx.scene.directory = os.path.dirname(os.path.abspath(__file__)) + "/Resources/"

    grid = mx.scene.add_grid("Grid", 20)
    grid.scale(0.5, 0.5, 0.5)
    grid.move_up(-0.5)

    arc = mx.scene.load_object("Arc", "objects/arc170/arc170.obj")
    arc.scale(0.001, 0.001, 0.001)
    arc.move_forward(2.0).move_right(-2.0)
    arc.rotate(radians(180), 0)
    
    arc.make_instanced(3)

    dist = 0.0
    for a in arc.instances:
        a.transform.move_x(dist)
        dist += 2.0

    mx.scene.viewport.perspective()
    #mx.scene.point_lights.resize(1)
    mx.scene.spot_lights.resize(1)

    mx.scene.global_light.ambient   = vec3(0.1)
    mx.scene.global_light.diffuse   = vec3(0.2)
    mx.scene.global_light.specular  = vec3(0.5)
    mx.scene.global_light.direction = vec3(1.0, 1.0, 0.0)
    
    mx.scene.spot_lights[0].ambient   = vec3(0.1, 0.0,  0.0)
    mx.scene.spot_lights[0].diffuse   = vec3(0.8, 0.3,  0.0)
    mx.scene.spot_lights[0].specular  = vec3(0.8, 0.3,  0.0)
    mx.scene.spot_lights[0].position  = vec3(2.0, 1.0,  0.0)
    mx.scene.spot_lights[0].direction = vec3(0.7, 1.0, -1.0)

    spot_lights_binder(mx.scene.spot_lights).bind()
    point_lights_binder(mx.scene.point_lights).bind()

    control = control_binder("control", mx.scene.viewport)
    control.bind_move(keycode.W, keycode.A, keycode.S, keycode.D, keycode.SPACE, keycode.LEFT_SHIFT)
    control.bind_rotate()
    bind_console("console", keycode.GRAVE_ACCENT)
    bind_close("close", keycode.ESCAPE)

    mx.listen_fps("print", lambda e: mx.log.debug("py-user", str(e.fps) + " FPS"))

def on_update():
    pass

mx = create_application()
mx.create_context()

mx.on_create = on_create
mx.on_update = on_update

mx.run()
destroy_application()
# D:/cpplibs/python/python.exe D:/repos/MxEngine/examples/PythonApplication/PythonApplication.py