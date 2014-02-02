qrush
=====

Qrush!

## Handler Functions

Declare these functions in your script. They will be automatically called when the corresponding event is triggered.

```javascript
function onClick(x, y){
  // draw stuff
}
function onDrag(x, y){
  // draw stuff
}
function onRelease(x, y){
  // draw stuff
}
```

`x` and `y` describe the current position of the mouse. 

## Utility functions

```javascript
log(msg); // logs the specified message
getColorData([refresh], [type]); // gets an array containing the color values for all visible pixels of the canvas
getCanvasWidth(); // get the current width of the canvas
getCanvasHeight(); // get the current height of the canvas
```

### getColorData

`getColorData` returns an Array containing containing the color values for all visible pixels of the canvas.

The function has two optional parameters:

* `refresh` is a Boolean that specifies whether Qrush should fill the array with the latest color values. If set to *false*, you will receive the array that was created when `getColorData` was last called with *true*. **Setting the parameter to *true* can notably slow down your application, depending on the `type` of data you expect and how often the function is called.** Defaults to *false*.

* `type` can be `"number"`, `"array"`, or `"object"` and specifies the type of content in the array that is returned to you. Defaults to `"number"`. Be aware that `"array"` and `"object"` are currently much slower than `"number"`, especially if `refresh` is set to *true*.


## Drawing functions

```javascript
point(x, y, color);
line(x1, y1, x2, y2, color);
rect(x1, y1, x2, y2, color, fill);
circle(x, y, r, color, fill);
ellipse(x1, y1, x2, y2, color, fill);
```

`color` can be a Number like `0x00FF00FF`, an Array like `[0,255,0, 128]` or an Object like `{r:0,g:255,b:0, a:255}`.
Notice that you have to specify an alpha otherwise your alpha value will be set to 0 by default.

`fill` is a Boolean that specifies whether you want the shape filled.
