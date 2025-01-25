public class InvalidClassSignature {
    public static void main(String[] a) {
        System.out.println(new MyClass().Init());
    }
}

class MyClass=myClass // @error - syntax (invalid class signature)
{

    public boolean Init() {
        Age = v_Age;
        return true;
    }

}
