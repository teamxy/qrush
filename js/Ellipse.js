var color = 0xffffffff;
var preColor = 0xff333333;

var x0 = 0;
var y0 = 0;

var onClick = function (x, y) {
	x0 = x;
	y0 = y;
}

var onDrag = function (x, y) {
	setPreview(true);
	fill(0);
	ellipse(x0, y0, x, y, preColor);
}

var onRelease = function (x, y) {

	setPreview(false);
	ellipse(x0, y0, x, y, color, true);

}