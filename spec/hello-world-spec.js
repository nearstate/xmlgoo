var buster = require( "buster" ),
	xmlgoo = require( ".." )
	;

buster.spec.expose();

describe( "Given xml", function() {

	before( function() {

		this.xml = "<root><hello>world</hello></root>";

	} );

	describe( "When parsed", function() {

		before( function() {

			this.result = xmlgoo.parse(this.xml);

		} );

		it( "it should create a document with an xml property returning the same xml", function() {

			expect( this.result.xml() ).toEqual( this.xml );

		} );

	} );

} );