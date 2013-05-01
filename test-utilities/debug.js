var goo = require("..");


var totalIterations = 100000;
var checkPoint = 1000;
var xmldata = "<hello>" +
	"world<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
	"<hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello><hello>world</hello>" +
"</hello>";
var i = 0;
var lg = false;

function run() {

	"use strict";

	if(lg) console.log("---");
	if(i % checkPoint === 0) {
		gc();
		console.log(i, process.memoryUsage());
	}

	var doc2 = goo.parse("<baseline />");
	var test1 = doc2.createElement("test1");
	var test2 = doc2.createElement("text2");
	test1.appendChild(test2);

	console.log("xdoc");
	var xdoc = goo.parse("<hello><world>bah</world><la>la</la></hello>");
	console.log("ydoc");
	var ydoc = goo.parse("<why><hello>there</hello></why>");
	var yhello = ydoc.documentElement().selectSingleNode("//hello");

	console.log("ydoc near death");
	ydoc = null;
	gc();

	console.log("yhello (ydoc's pin) changing");
	yhello.changeOwnerTo(xdoc);
	gc();

	xdoc.documentElement().appendChild(yhello);
	gc();

	console.log("yhello near death");
	yhello = null;
	gc();

	var el1 = xdoc.createElement("hello");
	console.log(1);
	var el2 = xdoc.createElement("hello2");
	console.log(2);
	el1.appendChild( el2 );
	console.log(3);

	var yoyo = xdoc.documentElement().selectSingleNode("//world");
	var la = xdoc.documentElement().selectSingleNode("//la");

	for(var j = 0; j < 2; j++)
		xdoc.documentElement().childNodes()[0].appendChild(
			xdoc.documentElement().childNodes()[1].cloneNode( true )
		);

	gc();


	var world = xdoc.documentElement().selectSingleNode("//world");
	console.log("gc document element");
	gc();
	var clone = world.cloneNode(true);
	var hello = xdoc.documentElement();
	var lala = xdoc.documentElement().selectSingleNode("/hello/la");
	hello.removeChild(lala);


	console.log("gc world");
	world = null;
	gc();
	console.log("gc clone");
	clone = null;
	gc();

	console.log("gc doc element and doc");
	xdoc = null;
	hello = null;
	gc();

	console.log("gc removed");
	la = null;
	gc();

	if(lg) console.log("\nCreating document");
	var x = goo.parse(xmldata);

	if(lg) console.log("\nReading document");
	if(lg) console.log(x.xml());

	if(lg) console.log("\nGetting element");
	var el = x.documentElement();


	if(lg) console.log("\nNulling document");
	x = null;
	if(lg) console.log("doc is now near death");

	if(lg) console.log("\nReading element");
	if(lg) console.log(el.xml());

	if(lg) console.log("\nGetting element ownerDocument");
	var od = el.ownerDocument();

	if(lg) console.log("\nComparing ownerDocument documentElement with documentElement");
	if( od.documentElement() !== el ) throw new Error("No match");


	if(lg) console.log("\nGetting element childNodes");
	var cn = el.childNodes();
	if(lg) console.log(cn.length);

	if(lg) console.log("\nNulling element");
	el = null;
	if(lg) console.log("el is now near death");

	if(lg) console.log("\nParsing and getting documentElement in one operation");
	var aio = goo.parse("<hello>there<my>pretties</my></hello>").documentElement();

	var doc = goo.parse("<hello><world /></hello>");
	gc();
	var cloned = doc.documentElement().childNodes()[0].cloneNode(true);
	gc();
	doc.documentElement().appendChild(cloned);
	doc = null;
	gc();

}

console.log(totalIterations, i);

for(i = 0; i < totalIterations; i++) run();

console.log(i);

if(lg) console.log("-----\n\n");
gc();
if(lg) console.log("-----\n\n");
gc();
if(lg) console.log("-----\n\n");
gc();
if(lg) console.log("-----\n\n");
gc();

console.log(process.memoryUsage());

setTimeout(function() {
	if(lg) console.log("-----\n\n");
	gc();
	if(lg) console.log("end");
}, 100);
