var buster = require( "buster" ),
	xmlgoo = require( ".." )
	;

buster.spec.expose();

describe( "Given node from parsed tree", function() {

	before( function() {

		this.testXml = "<root><hello thisIs=\"something new\">world</hello><one><two/><three>hello there</three><four/></one></root>";
		this.doc = xmlgoo.parse( this.testXml );
		this.elementOne = this.doc.documentElement().childNodes()[1];

	} );

	it( "Should return the correct nodeType", function() {

		expect( this.elementOne.nodeType() ).toEqual( 1 );

	} );

	it( "Should return the correct nodeName", function() {

		expect( this.elementOne.nodeName() ).toEqual( "one" );

	} );

	it( "Should return the correct nodeValue", function() {

		expect( this.elementOne.nodeValue() ).toEqual( "hello there" );

	} );

	it( "Should return the correct attribute value", function() {

		expect( this.doc.documentElement().childNodes()[0].getAttribute( "thisIs" ) ).toEqual( "something new" );

	} );

	it( "Should correctly return for a non-existant attribute value", function() {

		expect( this.doc.documentElement().childNodes()[0].getAttribute( "thisIsnt" ) ).toBeFalsy();

	} );

	it( "Should correctly return if an attribute exists", function() {

		expect( this.doc.documentElement().childNodes()[0].hasAttribute( "thisIs" ) ).toBeTrue();

	} );

	it( "Should correctly return if an attribute doesn't exist", function() {

		expect( this.doc.documentElement().childNodes()[0].hasAttribute( "thisIsnt" ) ).toBeFalse();

	} );

} );