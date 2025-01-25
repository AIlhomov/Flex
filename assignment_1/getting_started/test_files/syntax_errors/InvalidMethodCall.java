public class InvalidMethodCall {
    public static void main(String[] a) {
        System.out.println(new MyClass().Init());
    }
}

class MyClass {
    public boolean Init(){
        int test;

        test = this.Bar(a,); //@error - syntax (trailing comma in the list of parameters)
        return false;
    }

    public int Bar(int a, ) {
        return 1;
    }
}
