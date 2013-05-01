var buster = require( "buster" ),
	xmlgoo = require( ".." )
	;

buster.spec.expose();

describe( "Given an execution context other than a document", function() {

	before( function() {

		this.target = {};
		this.methodSource = xmlgoo.parse("<hello />");

	} );

	testInvalid( "createElement", [ "test" ], "Illegal invocation" );
	testInvalid( "createTextNode", [ "test" ], "Illegal invocation" );
	testInvalid( "documentElement", [ ], "Illegal invocation" );
	testInvalid( "xml", [ ], "Illegal invocation");

});

describe( "Given an execution context other than a node", function() {

	before( function() {

		this.target = {};
		this.methodSource = xmlgoo.parse("<hello />").documentElement();

	} );

	testInvalid( "appendChild", [ ], "Illegal invocation" );
	testInvalid( "cloneNode", [ ], "Illegal invocation" );
//	testInvalid( "changeOwnerTo", [ ], "Illegal invocation" );
	testInvalid( "childNodes", [ ], "Illegal invocation" );
	testInvalid( "getAttribute", [ ], "Illegal invocation" );
	testInvalid( "hasAttribute", [ ], "Illegal invocation" );
	testInvalid( "nextSibling", [ ], "Illegal invocation" );
	testInvalid( "nodeName", [ ], "Illegal invocation" );
	testInvalid( "nodeType", [ ], "Illegal invocation" );
	testInvalid( "nodeValue", [ ], "Illegal invocation" );
	testInvalid( "xml", [ ], "Illegal invocation" );
	testInvalid( "ownerDocument", [ ], "Illegal invocation" );
	testInvalid( "parentNode", [ ], "Illegal invocation" );
	testInvalid( "previousSibling", [ ], "Illegal invocation" );
	testInvalid( "removeChild", [ ], "Illegal invocation" );
	testInvalid( "selectNodes", [ ], "Illegal invocation" );
	testInvalid( "selectSingleNode", [ ], "Illegal invocation" );
	testInvalid( "setAttribute", [ ], "Illegal invocation" );

});


describe( "Given a text node", function() {

	before( function() {

		this.target = xmlgoo.parse("<hello>some text</hello>").documentElement().childNodes()[0];
		this.methodSource = this.target;

	} );

	var anElement = xmlgoo.parse("<hi />").documentElement();


	testInvalid( "appendChild", [ anElement ], "can only be appended to an element" );
	testInvalid( "childNodes", [ ], "on an element" );
	testInvalid( "getAttribute", [ "key" ], "on an element" );
	testInvalid( "hasAttribute", [ "key" ], "on an element" );
	testInvalid( "removeChild", [ anElement ], "on an element" );
	testInvalid( "setAttribute", [ "key", "value" ], "on an element" );

});

describe( "Given the document element", function() {

	before( function() {

		this.target = xmlgoo.parse("<hello />").documentElement();
		this.methodSource = this.target;

	} );

	var aDocument = xmlgoo.parse("<hi />");

//	testInvalid( "changeOwnerTo", [ aDocument ], "document element" );
	testInvalid( "nextSibling", [ ], "document element" );
	testInvalid( "previousSibling", [ ], "document element" );

});

function testInvalid(funcName, args, expectedMessage) {

	describe( "When " + funcName + " is called", function() {

		before( function() {

			try {
				this.result = this.methodSource[funcName].apply( this.target, args );
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