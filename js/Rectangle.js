var x0, y0;

var color = 0x770000FF;

var onClick = function (x, y) {
	x0 = x;
	y0 = y;
	log("click");
}

var onDrag = function (x, y) {
	setPreview(true);
	fill(0);
	rect(x0, y0, x, y, color);
}

var onRelease = function (x, y) {
	setPreview(false);
	rect(x0, y0, x, y, color, true);
}