var buster = require( "buster" ),
	xmlgoo = require( ".." )
	;

buster.spec.expose();

describe( "Given node to manipulate", function() {

	before( function() {

		this.testXml = "<root><hello thisIs=\"something new\">world</hello><one><two/><three>hello there</three><four/></one></root>";
		this.doc = xmlgoo.parse( this.testXml );
		this.elementHello = this.doc.documentElement().childNodes()[0];
		this.elementOne = this.doc.documentElement().childNodes()[1];

	} );

	after( function() {

		gc();

	} );

	describe( "When setAttribute is called on an existing attribute", function() {

		before( function() {

			this.elementHello.setAttribute("thisIs", "something old");

		} );

		it( "it should update the value", function() {

			expect( this.elementHello.getAttribute("thisIs") ).toEqual( "something old" );

		} );

	} );

	describe( "When setAttribute is called on a non-existing attribute", function() {

		before( function() {

			this.elementHello.setAttribute("thatWas", "something old");

		} );

		it( "it should create the attribute", function() {

			expect( this.elementHello.getAttribute("thatWas") ).toEqual( "something old" );

		} );

	} );

	describe( "When appendChild is called", function() {

		before( function() {

			this.elementHello.appendChild( this.elementOne.cloneNode(true) );

		} );


		it( "it should have appended the node", function() {

			var kids = this.elementHello.childNodes();
			expect( kids.length ).toEqual( 2 ); // because the text node "world" also counts
			expect( kids[1].nodeName() ).toEqual( "one" );

		} );

	} );

	describe( "When removeChild is called with a node to remove", function() {

		before( function() {

			this.doc.documentElement().removeChild( this.elementHello );

		} );

		it( "it should have removed the node", function() {

			expect( this.doc.documentElement().selectNodes("//hello").length ).toEqual( 0 );

		} );

		describe( "And when we remove it again", function() {

			before( function() {

				try {
					this.doc.documentElement().removeChild( this.elementHello );
				} catch(e) {
					this.caught = e;
				}

			} );

			it( "it should have thrown an exception", function() {

				expect( this.caught ).toBeTruthy();

			} );

		} );

	} );

	describe( "When appendChild is called with a foreign node", function() {

		before( function() {

			this.foreignDocument = xmlgoo.parse("<hi>there</hi>");
			this.foreignNode = this.foreignDocument.documentElement();
			try {
				this.elementHello.appendChild( this.foreignNode );
			} catch( e ) {
				this.caught = e;
			}

		} );


		it( "it should throw an exception", function() {

			expect( this.caught ).toBeTruthy();

		} );

	} );

	describe( "When appendChild is called with a cloned foreign node", function() {

		before( function() {

			try {
				this.elementHello.appendChild( xmlgoo.parse("<hi>there</hi>").documentElement().cloneNode(true) );
			} catch(e) {
				this.caught = e;
			}

		} );


		it( "it should throw an exception", function() {

			expect( this.caught ).toBeTruthy();

		} );

	} );

	describe( "When a child is appended to itself", function() {

		before( function() {

			try {
				var doc = xmlgoo.parse("<hi><there>sam</there></hi>");
				var docElement = doc.documentElement();
				var thereElement = docElement.selectSingleNode("//there");
				thereElement.appendChild( docElement );

			} catch(e) {
				this.caught = e;
			}

		} );

		it( "it should throw an exception", function() {

			expect( this.caught ).toBeTruthy();

		} );

	} );

	describe( "When appendChild is called with a created text node", function() {

		before(function() {

			this.elementHello.appendChild( this.doc.createTextNode( "goodbye" ) );

		} );

		it( "it should have added the text content", function() {

			expect( this.elementHello.nodeValue() ).toEqual( "worldgoodbye" );

		} );

	} );

	describe( "When document.cloneNode is called on for an element in another document, and the cloned element is appended to the document which made the clone", function() {

		before( function() {

			this.toClone = this.doc.documentElement().selectSingleNode( "//one" );
			this.destinationDocument = xmlgoo.parse( "<hello />" );
			this.cloned = this.destinationDocument.cloneNode( this.toClone );
			this.destinationDocument.documentElement().appendChild( this.cloned );

		} );

		it( "a copy should be created within the document which made the clone", function() {

			expect( this.destinationDocument.documentElement().selectSingleNode("/hello/one/two") ).toBeTruthy();

		} );

		it( "the original node should remain in place", function() {

			expect( this.doc.documentElement().selectSingleNode("//one/two") ).toBe( this.toClone.childNodes()[0] );

		} );

	} );

	describe( "When insertBefore is called with a created element", function() {

		before( function() {

			this.createdElement = this.doc.createElement( "MovingInAtTheTop" );
			this.createdElement.insertBefore( this.doc.documentElement().childNodes()[0] );

		} );

		it( "it should have appended the node", function() {

			expect( this.doc.documentElement().childNodes()[0] ).toBe( this.createdElement );

		} );

	} );

} );