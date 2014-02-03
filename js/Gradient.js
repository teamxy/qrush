var color;
var size = 10;

function onClick(x,y){
  color = {r:0,g:20,b:0, a:255};
}

function onDrag(x,y){
  if(color.g !== 255){
  	color.g = color.g + 1;
  }
  circle(x, y, size, color, true);
}