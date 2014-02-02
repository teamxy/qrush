var x0, y0;

var onClick = function (x, y) {
	x0 = x;
	y0 = y;
}

var onDrag = function (x, y) {
	line(x0, y0, x, y, 0xff000000);
	x0 = x;
	y0 = y;
}

