public class SemanticMethodCallInBooleanExpression {
    public static void main(String[] a) {
        System.out.println(new A().a2());
    }
}

class A {
    boolean b;

    public A a1(int num) {
        int num_aux;
        if (this.a3())
            num_aux = 1;
        else if (this.a2() < 1)
            num_aux = num * (this.a2());
        else
            num_aux = 0;
        return this;
    }

    public int a2() {
        b = this.a1(this.a1(2).a4()).a3();
        int ret;
        if (b)
            ret = 1;
        else
            ret = 0;
        return ret;
    }

    public boolean a3() {
        return true;
    }

    public int a4() {
        return 1;
    }

}
