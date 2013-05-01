var buster = require( "buster" ),
	xmlgoo = require( ".." )
	;

buster.spec.expose();

describe( "Given document to create nodes from", function() {

	before( function() {

		this.testXml = "<root><hello thisIs=\"something new\">world</hello><one><two/><three>hello there</three><four/></one></root>";
		this.doc = xmlgoo.parse( this.testXml );
		this.elementHello = this.doc.documentElement().childNodes()[0];
		this.elementOne = this.doc.documentElement().childNodes()[1];

	} );

	describe( "When cloneNode(false) is called on a node with children", function() {

		before( function() {

			this.cloned = this.elementOne.cloneNode(false);

		} );

		it( "it should clone the node", function() {

			expect( this.cloned.nodeName() ).toEqual( "one" );

		} );

		it( "it should not clone any children", function() {

			expect( this.cloned.childNodes().length ).toEqual( 0 );

		} );

		addNewNodeTests("cloned", false, false);

	} );

	describe( "When cloneNode(true) is called on a node with children", function() {

		before( function() {

			this.cloned = this.elementOne.cloneNode(true);

		} );

		it( "it should clone the node", function() {

			expect( this.cloned.nodeName() ).toEqual( "one" );

		} );

		it( "it should clone all children", function() {

			expect( this.cloned.childNodes().length ).toEqual( 3 );

		} );

		addNewNodeTests("cloned", true, false);

	} );

	describe( "When createElement is called on the doc", function() {

		before( function() {

			this.created = this.doc.createElement("yoda");

		} );

		it( "it should create the element", function() {

			expect( this.created.nodeName() ).toEqual( "yoda" );

		} );

		addNewNodeTests("created", false, false);

	} );

	describe( "When createTextNode is called on the doc", function() {

		before( function() {

			this.created = this.doc.createTextNode("hello world");

		} );

		it( "it should create the text node", function() {

			expect( this.created.xml() ).toEqual( "hello world" );

		} );

		addNewNodeTests("created", true, true);

	} );

	function addNewNodeTests(nodeKey, containsTextValue, isTextNode) {

		var nodeTypeName = (isTextNode ? " text node" : " element");


		describe( "And ownerdocument is called on the " + nodeKey + " " + nodeTypeName, function() {

			before( function() {

				this.nodeOwnerDocument = this[nodeKey].ownerDocument();

			} );

			it( "it should return the owner document", function() {

				expect( !!~this.nodeOwnerDocument ).toBeTruthy();

			} );

		} );

		describe( "And parentNode is called on the " + nodeKey + " " + nodeTypeName, function() {

			before( function() {

				this.nodeParentNode = this[nodeKey].parentNode();

			} );

			it( "it should return undefined", function() {

				expect( this.nodeParentNode ).not.toBeDefined();

			} );

		} );

		describe( "And xml is called on the " + nodeKey + " " + nodeTypeName, function() {

			before( function() {

				this.nodeXml = this[nodeKey].xml();

			} );

			it( "it should return some xml", function() {

				expect( this.nodeXml ).toMatch( isTextNode ? /.+/ : /<.*>/ );

			} );

		} );

		describe( "And nodeValue is called on the " + nodeKey + " " + nodeTypeName, function() {

			before( function() {

				this.nodeValue = this[nodeKey].nodeValue();

			} );

			if( containsTextValue ) {

				it( "it should return value", function() {

					expect( this.nodeValue ).toBeTruthy();

				} );

			} else {

				it( "it should not return the value", function() {

					expect( this.nodeValue ).toEqual( "" );

				} );
			}

		} );


		describe( "And nodeType is called on the " + nodeKey + " " + nodeTypeName, function() {

			before( function() {

				this.nodeType = this[nodeKey].nodeType();

			} );

			it( "it should return XML_ELEMENT_NODE (1)", function() {

				expect( this.nodeType ).toEqual( isTextNode ? 3 : 1 );

			} );

		} );

		describe( "And nextSibling or previousSibling are called on the " + nodeKey + " " + nodeTypeName, function() {

			before( function() {

				this.nodeNextSibling = this[nodeKey].nextSibling();
				this.nodePreviousSibling = this[nodeKey].previousSibling();

			} );

			it( "it should return undefined", function() {

				expect( this.nodeNextSibling ).not.toBeDefined();
				expect( this.nodePreviousSibling ).not.toBeDefined();

			} );

		} );

		describe( "And hasAttribute or getAttribute are called on the " + nodeKey + " " + nodeTypeName, function() {

			before( function() {


				try { this.nodeHasAttribute = this[nodeKey].hasAttribute("hello"); } catch( e ) { this.nodeHasAttributeCaught = e; }
				try { this.nodeGetAttribute = this[nodeKey].getAttribute("world"); } catch( e ) { this.nodeGetAttributeCaught = e; }

			} );

			if( isTextNode ) {

				it( "it should throw", function() {

					expect( this.nodeHasAttributeCaught ).toBeTruthy();
					expect( this.nodeGetAttributeCaught ).toBeTruthy();

				});

			} else {

				it( "it should return false and undefined respectively", function() {

					expect( this.nodeHasAttribute ).toEqual( false );
					expect( this.nodeGetAttribute ).not.toBeDefined();

				} );

			}

		} );

		describe( "And selectNodes or selectSingleNode is called on the " + nodeKey + " " + nodeTypeName, function() {

			before( function() {

				try { this.nodeSelectNodes = this[nodeKey].selectNodes("//*"); } catch( e ) { this.nodeSelectNodesCaught = e; }
				try { this.nodeSelectSingleNode = this[nodeKey].selectSingleNode("/*[1]"); } catch( e ) { this.nodeSelectSingleNodeCaught = e; }

			} );

			it( "it should return selected nodes", function() {

				expect( this.nodeSelectNodes.length > 0 ).toBe( true, "select nodes" );
				expect( this.nodeSelectSingleNode ).toBeTruthy( "select single node" );

			} );

		} );

		describe( "And setAttribute is called on the " + nodeKey + " " + nodeTypeName, function() {

			before( function() {

				try { this[nodeKey].setAttribute("hello", "world"); } catch(e) { this.setAttributeCaught = e; }

			} );

			if( isTextNode ) {

				it( "it should have thrown", function() {

					expect( this.setAttributeCaught ).toBeTruthy();

				} );

			} else {

				it( "it should have set the attribute value", function() {

					expect( this[nodeKey].xml() ).toMatch( /hello="world"/ );

				} );

			}

		} );

		describe( "And appendChild is called on the " + nodeKey + " " + nodeTypeName, function() {

			before( function() {

				this.toAdd = this.doc.createElement("toAdd");
				try { this[nodeKey].appendChild(this.toAdd); } catch(e) { this.appendChildCaught = e; }

			} );

			if( isTextNode ) {

				it( "it should have thrown", function() {

					expect( this.appendChildCaught ).toBeTruthy();

				} );

			} else {

				it( "it should have added the element", function() {

					expect( this.appendChildCaught ).not.toBeDefined();
					expect( !!~this[nodeKey].childNodes().indexOf( this.toAdd ) ).toBe( true, "Added element not found" );

				} );

			}

		} );

	}

} );