public class MethodMissingClosingBracket {
    public static void main(String[] a) {
        System.out.println(new MyClass().Init());
    }
}

class MyClass {
    public boolean Init(){
        Age = v_Age ;
        return true;
}
// @error - syntax (missing closing bracket)