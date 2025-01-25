public class IllegalIfInIfCondition {
    public static void main(String[] a) {
        System.out.println(new MyClass().Init());
    }
}

class MyClass {
    public boolean Init(){
        if(if(true)){ //@error - syntax (illegal boolean condition)
        }
    }
}
