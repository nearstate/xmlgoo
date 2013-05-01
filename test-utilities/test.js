var xmlgoo = require("..");

var i = 0;

function run() {
	var x = xmlgoo.parse("<root>hello<world>test</world></root>");
	var y = x.xml();
	var d = x.documentElement();
	y = d.xml();
	var o = d.ownerDocument();
	var t = d.nodeType();
	var cn = d.childNodes()[1].childNodes();
	var name = d.nodeName();
	var value = d.nodeValue();
	var aio = xmlgoo.parse("<hello>there<my>pretties</my></hello>").documentElement();
	var attributeValue = xmlgoo.parse("<hello there=\"world\" />").documentElement().getAttribute("world");
	var nonAttributeValue = xmlgoo.parse("<hello />").documentElement().getAttribute("blah");
	var isIt = xmlgoo.parse("<hello i=\"am\" />").documentElement().hasAttribute("i");
	var isntIt = xmlgoo.parse("<hello i=\"am\" />").documentElement().hasAttribute("you");
	var nodeParent = d.childNodes()[0].parentNode();
	var documentElementParent = d.parentNode();
	var family = xmlgoo.parse("<parent><son>bob</son><daughter>bobbie</daughter></parent>");
	var daughter = family.documentElement().childNodes()[0].nextSibling();
	var pet = daughter.nextSibling();
	var son = daughter.previousSibling();
	var ghost = family.documentElement().selectSingleNode("//ghost");
	var bobbie = family.documentElement().selectSingleNode("//daughter");
	var everyone = family.documentElement().selectNodes("//*");
	family.documentElement().setAttribute("something", "new");
	family.documentElement().setAttribute("something", "old");

	daughter.cloneNode(false);
	family.documentElement().cloneNode(true);
	family.documentElement().removeChild(son); // removing a child

	var txt = family.createTextNode("hello world");

	var somethingNew = family.createElement("pet");
	family.documentElement().appendChild(daughter.cloneNode(false));

	var anotherNewThing = family.createTextNode("sutfffff");
	family.documentElement().appendChild(anotherNewThing);

	var anotherDocument = xmlgoo.parse("<hello><I><hate>it</hate></I></hello>");
	var de = family.cloneNode(anotherDocument.documentElement().childNodes()[0]);
	family.documentElement().appendChild(de);

	var pet2 = family.createElement("giraffe");
	pet2.insertBefore(family.documentElement().childNodes()[0]);

	if(i % (totalIterations / 10) === 0) console.log(i, process.memoryUsage());
}

var totalIterations = 3000;
var iterations = totalIterations;

while(iterations-- > 0) {
	i++;
	run();
}

xmlgoo._disposeModule();
xmlgoo = null;

gc();
gc();
gc();
gc();
gc();
gc();
gc();
gc();
gc();
gc();
gc();
gc();
gc();
gc();
gc();
gc();
gc();
gc();
gc();
gc();