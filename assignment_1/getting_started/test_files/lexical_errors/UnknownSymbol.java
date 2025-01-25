public class UnknownSymbol {
    public static void main(String[] args) {
        System.out.println("Hello"); //@error - lexical ('"' symbol is not recognized by the grammar)
        System.out.println(status%); //@error - lexical ('%' symbol is not recognized by the grammar)
        System.out.println($invalid); //@error - lexical ('$invalid' is invalid identifier)
        boole@n flag; //@error - lexical ('@'' symbol is not recognized by the grammar)
    }
}