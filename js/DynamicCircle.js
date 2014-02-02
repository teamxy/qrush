var color = 0x30000066;
var radius = 0;

var onClick = function (x, y) {
	radius = 0;
}

var onDrag = function (x, y) {

	circle(x, y, radius, color, true);
	radius = Math.min(100, radius+1);
	

}