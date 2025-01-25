public class MissingReturnStatement {
    public static void main(String[] args) {
        System.out.println(new MyClass().test());
    }
}

class MyClass {
    public int test() {
        int a;
        a = 5;
        int b;
        b = 10;
    } // @error - syntax (Missing return statement)
}
