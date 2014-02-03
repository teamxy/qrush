var color = 0xffffffff;

var r = 0;
var cX = 0;
var cY = 0;

var onClick = function (x, y) {
	cX = x;
	cY = y;
	r = x;
}

var onDrag = function (x, y) {
	setPreview(true);
	fill(0);
	r = Math.abs(x - cX);
	circle(cX, cY, r, color);
}

var onRelease = function (x, y) {
	setPreview(false);
	circle(cX, cY, r, color, true);

}