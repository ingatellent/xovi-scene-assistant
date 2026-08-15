# reMarkable Scene Assistant for xovi

This is a xovi extension which adds the following functionality

- save the clipboard to a file
- read a saved clipboard from file and convert to scene items
- change the color, pen or thickness of scene items (e.g. from the clipboard)
- draw strokes on the current layer of the opened document

In order to handle saved clipboards, to be used as e.g. stickers, it also contains functionality to
- ensure that a certain directory exists
- delete a file (use carefully!)

Currently the methods for changing thickness of strokes does not change the bounding box, so thicker strokes will overflow the box (and be cut off).

It's not so difficult to also render strokes from .svg files, but one needs to parse the svg (or find a c++ svg parser to include).

PRs or suggestions are welcome, but any PR with unhashed qmd files will be rejected.

The clipboard interaction as well as the initial work regarding rm_Line, rm_SceneItem and rm_SceneLineItem was originally created by HookedBehemoth <https://github.com/HookedBehemoth/xovi-sudoku>. This work was released under GPL v2, and hence that is also the license of this repository.

The responsibility for the vtable discovery is moved from the QML side to the c++ side, making the use of this library much easier.

## Use in mods

To use the Scene Assistant in e.g. a qml project, you must import using
```
import dk.ingatellent.SceneAssistant 1.0
```

It is then possible to save scene items to a file (saved as a json file as well as a .svg preview) using
```
SceneAssistant.saveSceneItems(itemsToSave, filePath) 
```                   

You can delete a file with
```
SceneAssistant.deleteFile(filePath) 
```

You can create a directory with
```
SceneAssistant.ensureDirectory(dir)
```

You can load a .json file with items using
```
SceneAssistant.loadSceneItems(filePath,controller)
```
the items can then e.g. be added to the Clipboad or directly to the canvas. The second argument is optional, but needed if you haven't already setup the vtable. You need to pass a scene controller to setup the vtable - either using this method or using
```
SceneAssistant.ensureVtable(controller)
```

You can change strokes using the functions
```
setColorOnSceneItems(items, colorEnum, rgba)
setToolOnSceneItems(items, toolEnum)
setThicknessOnSceneItems(items, thickness)
increaseThicknessOnSceneItems(items)
decreaseThicknessOnSceneItems(items)
```

You can add a line or a box from end points or (opposite) vertices using the functions
```
createLine(start, end);
createSelectionBox(start, end);
```
