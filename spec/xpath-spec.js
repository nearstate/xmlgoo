var buster = require( "buster" ),
	xmlgoo = require( ".." )
	;

buster.spec.expose();

describe( "Given xml tree to select with xpath", function() {

	before( function() {

		this.testXml = "<root><hello to=\"be\">world</hello><one><two/><three/><four/></one></root>";
		this.doc = xmlgoo.parse( this.testXml );

	} );

	describe( "When single node is selected", function() {

		before( function() {

			this.one = this.doc.documentElement().selectSingleNode( "//one" );

		} );

		it( "it should be the right node", function() {

			expect( this.one ).toBe( this.doc.documentElement().childNodes()[1] );

		} );

	} );

	describe( "When a non-existent node is selected", function() {

		before( function() {

			this.fortyTwo = this.doc.documentElement().selectSingleNode( "/forty[2]" );

		} );

		it( "it should be undefined", function() {

			expect( this.fortyTwo ).not.toBeDefined();

		} );

	} );

	describe( "When many nodes are selected", function() {

		before( function() {

			this.all = this.doc.documentElement().selectNodes("//*");

		} );

		it( "it should return the right number of nodes", function() {

			expect( this.all.length ).toEqual( 6 );

		} );

		it( "it should return the same objects as obtained by traversing the tree", function() {

			expect( this.all[1] ).toBe( this.doc.documentElement().childNodes()[0] );

		} );

	} );

	describe( "When selectNodes matches nothing", function() {

		before( function() {

			this.nada = this.doc.documentElement().selectNodes("an honest man");

		} );

		it( "it should return an empty array", function() {

			expect( this.nada.length ).toEqual( 0 );
			expect( Object.prototype.toString.call(this.nada) ).toMatch( "Array" );

		} );

	} );

} );