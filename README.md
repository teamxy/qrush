qrush
=====

Qrush!

## Handler Functions

Declare these functions in your script. They will be automatically called when the corresponding event is triggered.

```javascript
function onClick(x, y, width, height){
  // draw stuff
}
function onDrag(x, y, width, height){
  // draw stuff
}
function onRelease(x, y, width, height){
  // draw stuff
}
```

`x` and `y` describe the current position of the mouse, `width` and `height` are the total size of the canvas. 

## Utility functions

```javascript
log(msg); // logs the specified message
getColorData(); // gets an array containing the color values for all visible pixels of the canvas
```

## Drawing functions

```javascript
point(x, y, color);
line(x1, y1, x2, y2, color);
rect(x1, y1, x2, y2, color, fill);
circle(x, y, r, color, fill);
ellipse(x1, y1, x2, y2, color, fill);
```
