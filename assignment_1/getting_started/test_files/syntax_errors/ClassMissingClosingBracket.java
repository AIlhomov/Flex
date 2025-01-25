public class ClassMissingClosingBracket {
    public static void main(String[] a) {
        System.out.println(new Element().no_method(10));
    }
}

class Element { 
    //@error - syntax (class missing closing bracket)