var buster = require( "buster" ),
	xmlgoo = require( ".." )
	;

buster.spec.expose();

describe( "Given the module", function() {

	before( function() {

		this.target = xmlgoo;

	} );

	testInvalid( "parse", [ null ], "Null", "Please pass a single string" );
	testInvalid( "parse", [ "" ], "empty string", "invalid" );
	testInvalid( "parse", [ "<blah" ], "invalid xml", "parser error" );
	testInvalid( "parse", [ ], "Nothing", "Please pass a single string" );
	testInvalid( "parse", [ 42 ], "A number", "a single string" );

});

describe( "Given a document", function() {

	before( function() {

		this.target = xmlgoo.parse( "<root><hello>world</hello><goodbye reason=\"tick tock\">for now</goodbye></root>" );

	} );

	testInvalid( "createElement", [ null ], "Null", "a single string" );
	testInvalid( "createElement", [ "" ], "an empty string", "name" );
	testInvalid( "createElement", [ "<><" ], "an invalid tag name", "invalid" );
	testInvalid( "createElement", [ ], "no parameters", "a single string" );
	testInvalid( "createElement", [ 42 ], "a number", "a single string" );

	testInvalid( "createTextNode", [ null ], "Null", "single string" );
	testInvalid( "createTextNode", [ ], "no parameters", "single string" );
	testInvalid( "createTextNode", [ 42 ], "a number", "a single string" );

} );

describe( "Given an element in a document", function() {

	before( function() {

		this.doc = xmlgoo.parse( "<root><hello>world</hello><goodbye reason=\"tick tock\">for now</goodbye></root>" );
		this.target = this.doc.documentElement().childNodes()[1];

	} );

	testInvalid( "appendChild", [ null ], "Null", "a valid node");
	testInvalid( "appendChild", [ "" ], "an empty string", "a valid node");
	testInvalid( "appendChild", [ ], "no parameters", "a valid node");
	testInvalid( "appendChild", [ 42 ], "a number", "a valid node");
	testInvalid( "appendChild", function() { return [ this.target ]; }, "itself", "itself");
	testInvalid( "appendChild", function() { return [ this.doc.documentElement() ]; }, "its parent", "ancestor");
	testInvalid( "appendChild", function() { return [ xmlgoo.parse("<a><b>c</b></a>").documentElement().childNodes()[0] ]; }, "a foreign node", "same document");
	testInvalid( "appendChild", [ {} ], "an object other than a node", "a valid node");

	testInvalid( "cloneNode", [ null ], "Null", "true or false");
	testInvalid( "cloneNode", [ "" ], "an empty string", "true or false");
	testInvalid( "cloneNode", [ ], "no parameters", "true or false");
	testInvalid( "cloneNode", [ 42 ], "a number", "true or false");

	// testInvalid( "changeOwnerTo", [ null ], "Null", "expects a document" );
	// testInvalid( "changeOwnerTo", [ "" ], "an empty string", "expects a document");
	// testInvalid( "changeOwnerTo", [ ], "no parameters", "expects a document");
	// testInvalid( "changeOwnerTo", [ 42 ], "a number", "expects a document");
	// testInvalid( "changeOwnerTo", function() { return [ this.doc.documentElement() ]; }, "an element", "expects a document");

	testInvalid( "getAttribute", [ null ], "Null", "attribute name" );
	testInvalid( "getAttribute", [ "" ], "an empty string", "attribute name");
	testInvalid( "getAttribute", [ ], "no parameters", "attribute name");
	testInvalid( "getAttribute", [ 42 ], "a number", "attribute name" );

	testInvalid( "hasAttribute", [ null ], "Null", "attribute name" );
	testInvalid( "hasAttribute", [ "" ], "an empty string", "attribute name");
	testInvalid( "hasAttribute", [ ], "no parameters", "attribute name");
	testInvalid( "hasAttribute", [ 42 ], "a number", "attribute name" );

	testInvalid( "removeChild", [ null ], "Null", "a valid node");
	testInvalid( "removeChild", [ "" ], "an empty string", "a valid node");
	testInvalid( "removeChild", [ ], "no parameters", "a valid node");
	testInvalid( "removeChild", [ 42 ], "a number", "a valid node");
	testInvalid( "removeChild", function() { return [ this.doc.documentElement().childNodes()[0] ]; }, "a sibling", "from its parent");
	testInvalid( "removeChild", function() { return [ this.target ]; }, "itself", "from its parent");
	testInvalid( "removeChild", function() { return [ this.doc.documentElement() ]; }, "its parent", "from its parent");
	testInvalid( "removeChild", function() { return [ xmlgoo.parse("<a><b>c</b></a>").documentElement().childNodes()[0] ]; }, "a foreign node", "from its parent");
	testInvalid( "removeChild", [ {} ], "an object other than a node", "a valid node");

	testInvalid( "selectNodes", [ null ], "Null", "a valid xpath expression");
	testInvalid( "selectNodes", [ ], "no parameters", "a valid xpath expression");
	testInvalid( "selectNodes", [ 42 ], "a number", "a valid xpath expression");

	testInvalid( "selectSingleNode", [ null ], "Null", "a valid xpath expression");
	testInvalid( "selectSingleNode", [ ], "no parameters", "a valid xpath expression");
	testInvalid( "selectSingleNode", [ 42 ], "a number", "a valid xpath expression");

	testInvalid( "setAttribute", [ ], "No parameters", "the attribute name and value to set");
	testInvalid( "setAttribute", [ "tuple" ], "One parameter", "the attribute name and value to set");
	testInvalid( "setAttribute", [ null, "value" ], "Null attribute name", "string as the attribute name");
	testInvalid( "setAttribute", [ "", "value" ], "an empty string as attribute name", "string as the attribute name");
	testInvalid( "setAttribute", [ 42, "value" ], "a number as attribute name", "string as the attribute name");
	testInvalid( "setAttribute", [ "key", null ], "Null attribute value", "string as the value to set");
	testInvalid( "setAttribute", [ "key", "" ], "Null attribute value", "string as the value to set");
	testInvalid( "setAttribute", [ "key", 42 ], "Null attribute value", "string as the value to set");

	describe( "and a created element", function() {

		before( function() {

			this.target = this.doc.createElement("toInsert");
			this.target.appendChild(this.doc.createElement("toInsert_descendant"));

		} );

		testInvalid( "insertBefore", [ null ], "Null", "valid node" );
		testInvalid( "insertBefore", [ "" ], "an empty string", "valid node" );
		testInvalid( "insertBefore", [ ], "no parameters", "valid node" );
		testInvalid( "insertBefore", [ 42 ], "a number", "valid node" );
		testInvalid( "insertBefore", [ {} ], "a non-node", "valid node" );
		testInvalid( "insertBefore", function() { this.doc.documentElement().appendChild(this.target); return [ this.target ]; }, "itself", "itself");
		testInvalid( "insertBefore", function() { return [ this.doc.documentElement() ]; }, "the document element", "document element");
		testInvalid( "insertBefore", function() { return [ this.target.childNodes()[0] ]; }, "its child", "descendant");
		testInvalid( "insertBefore", function() { return [ this.target.childNodes()[0].cloneNode(false) ]; }, "a node which has no parent", "parent" );

	} );

} );


function testInvalid(funcName, args, argsDescription, expectedMessage) {

	describe( "When " + funcName + " is called with " + argsDescription, function() {

		before( function() {

			try {
				var thisArgs = ("function" == typeof args) ? args.call(this) : args;
				this.result = this.target[funcName].apply( this.target, thisArgs );
			} catch( e ) {
				this.caught = e;
			}

		} );

		it( "it should throw", function() {

			expect( this.caught ).toBeTruthy();

			if( expectedMessage ) {

				expect( this.caught.message ).toMatch( expectedMessage );
			}

		} );

		it( "it should not return a result", function() {

			expect( this.result ).not.toBeDefined( );

		} );

	} );

}