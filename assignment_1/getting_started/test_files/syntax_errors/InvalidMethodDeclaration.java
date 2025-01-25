public class InvalidMethodDeclaration {
    public static void main(String[] args) {
        System.out.println(new MyClass().testMethod());
    }
}

class MyClass {
    private void testMethod() { // @error - syntax (no methods of type void allowed)
        return a;
    }
}
