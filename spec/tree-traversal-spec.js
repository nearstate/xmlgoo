var buster = require( "buster" ),
	xmlgoo = require( ".." )
	;

buster.spec.expose();

describe( "Given xml tree to traverse", function() {

	before( function() {

		this.testXml = "<root><hello to=\"be\">world</hello><one><two/><three/><four/></one></root>";
		this.doc = xmlgoo.parse( this.testXml );

	} );

	describe( "When document element is requested", function() {

		before( function() {

			this.documentElement = this.doc.documentElement();

		} );

		it( "Should return a node with the correct content", function() {

			expect( this.documentElement.xml() ).toEqual( this.testXml );

		} );

		describe( "And the ownerDocument is requested", function() {

			before( function() {

				this.ownerDocument = this.documentElement.ownerDocument();

			} );

			it( "Should return the original document", function() {

				expect( this.ownerDocument ).toBe( this.doc );

			} );

			describe( "And the documentElement is requested again", function() {

				before( function() {

					this.de2 = this.ownerDocument.documentElement();

				} );

				it( "Should return the same document element", function() {

					expect( this.de2 ).toBe( this.documentElement );

				} );

			} );

		} );

		describe( "And the document element is requested again", function() {

			before( function() {

				this.de2 = this.doc.documentElement();

			} );

			it( "Should return the same node", function() {

				expect( this.de2 ).toBe( this.de2 );

			} );

		} );

		describe( "And the parent node is requested", function() {

			before( function() {

				this.deparent = this.documentElement.parentNode();

			} );

			it( "It should return the document", function() {

				expect( this.deparent ).toBe( this.doc );

			} );

		} );

		describe( "And childNodes is requested", function() {

			before( function() {

				this.documentElementChildNodes = this.documentElement.childNodes();

			} );

			it( "Should return an array of the two child nodes", function() {

				expect( this.documentElementChildNodes.length ).toEqual( 2 );
				expect( this.documentElementChildNodes[0].xml().substring( 0, 6 ) ).toEqual( "<hello" );

			} );

			describe( "And parentNode is requested of the first childNode", function() {

				before( function() {

					this.pn = this.documentElementChildNodes[0].parentNode();

				} );

				it( "Should return the document element", function() {

					expect( this.pn ).toBe( this.documentElement );

				} );

			} );

			describe( "And nextSibling is called on first child", function() {

				before( function() {

					this.nextSibling = this.documentElementChildNodes[0].nextSibling();

				} );

				it( "it should return the second child", function() {

					expect( this.nextSibling ).toBe( this.documentElementChildNodes[1] );

				} );

			} );

			describe( "And previousSibling is called on first child", function() {

				before( function() {

					this.previousSibling = this.documentElementChildNodes[0].previousSibling();

				} );

				it( "it should return undefined", function() {

					expect( this.previousSibling ).not.toBeDefined();

				} );

			} );

			describe( "And nextSibling is called on second child", function() {

				before( function() {

					this.nextSibling = this.documentElementChildNodes[1].nextSibling();

				} );

				it( "it should return undefined", function() {

					expect( this.nextSibling ).toBeFalsy();

				} );

			} );

			describe( "And previousSibling is called on second child", function() {

				before( function() {

					this.previousSibling = this.documentElementChildNodes[1].previousSibling();

				} );

				it( "it should return the first child", function() {

					expect( this.previousSibling ).toBe( this.documentElementChildNodes[0] );

				} );

			} );

			describe( "And childNodes is requested again", function() {

				before( function() {

					this.cn2 = this.documentElement.childNodes();

				} );

				it( "Should return an array of the same child nodes (by reference)", function() {

					expect( this.cn2[0] ).toBe( this.documentElementChildNodes[0] );

				} );

			} );

		} );

	} );


} );