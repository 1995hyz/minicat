"use strict";

var canvas;
var gl;
var block_length = 1;
var block_width = 0.5;
var block_height = 0.5;
var gap = 0.1;
var program;
var points = [];
var colors = [];
var NumVertices = 36;
var vertices = [
    vec4( -block_length/2-block_length-gap, -block_height/2-block_height-gap,  block_width/2+block_width+gap, 1.0 ),
    vec4( -block_length/2-block_length-gap,  block_height/2-block_height-gap,  block_width/2+block_width+gap, 1.0 ),
    vec4(  block_length/2-block_length-gap,  block_height/2-block_height-gap,  block_width/2+block_width+gap, 1.0 ),
    vec4(  block_length/2-block_length-gap, -block_height/2-block_height-gap,  block_width/2+block_width+gap, 1.0 ),
    vec4( -block_length/2-block_length-gap, -block_height/2-block_height-gap, -block_width/2+block_width+gap, 1.0 ),
    vec4( -block_length/2-block_length-gap,  block_height/2-block_height-gap, -block_width/2+block_width+gap, 1.0 ),
    vec4(  block_length/2-block_length-gap,  block_height/2-block_height-gap, -block_width/2+block_width+gap, 1.0 ),
    vec4(  block_length/2-block_length-gap, -block_height/2-block_height-gap, -block_width/2+block_width+gap, 1.0 )
];

const black = [ 0.0, 0.0, 0.0, 1.0 ];
const red = [ 1.0, 0.0, 0.0, 1.0 ];
const yellow = [ 1.0, 1.0, 0.0, 1.0 ];
const green = [ 0.0, 1.0, 0.0, 1.0 ];
const blue = [ 0.0, 0.0, 1.0, 1.0 ];
const magenta = [ 1.0, 0.5, 0.0, 1.0 ];

window.onload = function init() {
    canvas = document.getElementById( "gl-canvas" );
    gl = WebGLUtils.setupWebGL( canvas );
    if ( !gl ) { alert( "WebGL isn't available" ); }

    gl.viewport(0, 0, canvas.width, canvas.height);
    gl.clearColor(1.0, 1.0, 1.0, 1.0);

    gl.enable(gl.DEPTH_TEST);

    program = initShaders(gl, "vertex-shader", "fragment-shader");
    gl.useProgram(program);

    init_block();

    var cBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, cBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, flatten(colors), gl.STATIC_DRAW);
    var vColor = gl.getAttribLocation( program, "vColor" );
    gl.vertexAttribPointer( vColor, 4, gl.FLOAT, false, 0, 0 );
    gl.enableVertexAttribArray( vColor );

    var vBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, flatten(points), gl.STATIC_DRAW);
    var vPosition = gl.getAttribLocation( program, "vPosition" );
    gl.vertexAttribPointer( vPosition, 4, gl.FLOAT, false, 0, 0 );
    gl.enableVertexAttribArray( vPosition );

    render();
};

function init_block(){
    rectangleDrawer(1, 0, 3, 2, green);
    rectangleDrawer(2, 3, 7, 6, green);
    rectangleDrawer(3, 0, 4, 7, green);
    rectangleDrawer(6, 5, 1, 2, green);
    rectangleDrawer(4, 5, 6, 7, green);
    rectangleDrawer(5, 4, 0, 1, green);
}

function rectangleDrawer(a, b, c, d, color) {
    let indices = [a, b, c, a, c, d];
    for( let i = 0; i < 6; i++) {
        points.push(vertices[indices[i]]);
        colors.push((color));
    }
}

function render()
{

    gl.clear( gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.drawArrays( gl.TRIANGLES, 0, NumVertices );
    requestAnimFrame( render );
}