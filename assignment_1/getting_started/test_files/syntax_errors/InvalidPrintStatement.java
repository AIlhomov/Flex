public class InvalidPrintStatement {
    public static void main(String[] a) {
        System.out.println(new MyClass().Init());
    }
}

class MyClass {
    public boolean Init(){      
        System.out.println(int); //@error - syntax (invalid print statement)
    }
}
