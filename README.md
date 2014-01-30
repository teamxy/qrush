qrush
=====

Qrush!

Callback Functions
===================

Declare these functions in your script. They will be automatically called when the corresponding event is triggered.

```javascript
function onClick(x, y, width, height, data){
  // draw stuff
}
function onDrag(x, y, width, height, data){
  // draw stuff
}
function onRelease(x, y, width, height, data){
  // draw stuff
}
```

x and y describe the current position of the mouse, width and height are the total size of the canvas. data is an array containing the color values for all visible pixels of the canvas.

JavaScript Bindings
===================

```javascript
function point(x, y, color);
function line(x1, y1, x2, y2, color);
function rect(x1, y1, x2, y2, color, fill);
function circle(x, y, r, color, fill);
function ellipse(x1, y1, x2, y2, color, fill);
function log(msg);
```
