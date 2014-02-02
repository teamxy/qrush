var alpha = 0.0;

var triangle = function (x, y) {
	var size = Math.sin(alpha) * 20;
	var color = 0x66999999;

	alpha += 0.1;

	line(x - size, y - size, x, y + size, color);
	line(x + size, y - size, x, y + size, color);
	line(x - size, y - size, x + size, y - size, color);
}

var x0, y0;

var onDrag = function (x, y) {
	triangle(x, y);
}