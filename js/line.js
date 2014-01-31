var x0, y0;

function onRelease(x,y){
  x0 = undefined;
  y0 = undefined;
}

function onDrag(x,y){
var height = getCanvasHeight();
var pixels = getColorData();
if(x0 && y0){
  line(x0,y0,x, y, pixels[x * height + y] | 0x00FF00);
}
  x0 = x;
  y0 = y;
}