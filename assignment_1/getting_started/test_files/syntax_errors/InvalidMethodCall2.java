public class InvalidMethodCall3 {
    public static void main(String[] a) {
        System.out.println(new MyClass().Init());
    }
}

class MyClass {
    public boolean Init() {
        int test;

        test = this.Init; // @error - syntax (missing arguments)

        return false;
    }
}
