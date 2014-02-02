var color = 0x663399FF;
var alpha = 0;

var onClick = function (x, y) {
	alpha = 0;
}

var onDrag = function (x, y) {

	var radius = Math.sin(alpha) * 16;
	circle(x, y, radius, color, true);
	alpha += 0.1;

}