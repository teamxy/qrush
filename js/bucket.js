function onClick(x0,y0){
  var width = getCanvasWidth();
  var height = getCanvasHeight();
  for(var x=0; x < width; x++){
    for(var y=0; y < height; y++){
	point(x, y, 0xAF3300);
    }
  }
 var pixels = getColorData();
 log(pixels[3]);
}