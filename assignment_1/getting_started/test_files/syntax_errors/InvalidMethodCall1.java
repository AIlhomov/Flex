public class InvalidMethodCall1 {
    public static void main(String[] a) {
        System.out.println(new MyClass().Init());
    }
}

class MyClass {
    public boolean Init(){
        int test;

        test = this.Bar(,a); // @error - syntax (empty parameter before the comma in the list of parameters)
        return false;
    }

    public int Bar(, boolean b) {
        return 1;
    }
}
