var x0, y0;

function onRelease(x,y){
  x0 = undefined;
  y0 = undefined;
  var pixels = getColorData(true, "array");
}

function onDrag(x,y){
var height = getCanvasHeight();
var pixels = getColorData();
if(x0 && y0){
  line(x0,y0,x, y, [22,100,00]);
}
  x0 = x;
  y0 = y;
log(pixels[x * height + y]);
}