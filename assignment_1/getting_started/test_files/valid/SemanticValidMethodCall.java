public class SemanticValidMethodCall {
    public static void main(String[] a) {
        System.out.println(new A().a2());
    }
}

class A {

    public A a1(int num) {
        int num_aux;
        if (num < 1)
            num_aux = 1;
        else
            num_aux = num * (this.a3().a2());
        return this;
    }

    public int a2() {
        return 0;
    }

    public A a3() {
        A a;
        a = new A();
        return a;
    }

    public A a4() {
        A a;
        int i;
        a = this.a1(new A().a1(this.a2()).a2()).a3();
        i = this.a1(new A().a1(this.a2()).a2()).a2();
        a = this.a1(2).a3();

        int[] ia;
        i = this.a5(false, i, ia, this);

        return new A();
    }

    public int a5(boolean a, int b, int[] c, A d) {
        return 1;
    }

}
