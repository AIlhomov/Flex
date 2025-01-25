public class InvalidTypeDeclaration {
    public static void main(String[] a) {
        System.out.println(new MyClass().Init());
    }
}

class MyClass {
    public boolean Init(){
        int int; //@error - syntax (invalid type declaration)
        return true;
    }

}
