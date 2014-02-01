function onClick(x0,y0){
  var width = getCanvasWidth();
  var height = getCanvasHeight();
  for(var x=0; x < width; x++){
    for(var y=0; y < height; y++){
	//point(x, y, 0xAFFF00);
	//point(x,y, {g:32, b:200});
	point(x,y, [100,35,0]);
    }
  }
}