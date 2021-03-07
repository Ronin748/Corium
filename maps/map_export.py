# this script is used to export chunks from Blender

import bpy

objects = bpy.data.objects
for object in objects:
        bpy.ops.object.select_all(action='DESELECT')
        object.select = True
        exportName = './chunks/' + object.name + '.obj'
        bpy.ops.export_scene.obj(filepath=exportName, use_selection=True)
