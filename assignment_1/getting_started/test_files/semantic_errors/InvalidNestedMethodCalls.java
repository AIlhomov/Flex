public class InvalidNestedMethodCalls {
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
        b = this.a1(new A().a1(this.a3()).a4()).a3(); // @error - semantic (invalid parameter method 'a1')
        b = this.a1(new A().a1(2)).a3(); // @error - semantic (invalid method parameter for 'a1')
        b = this.a1(new A().a1(2).a3()).a3(); // @error - semantic (invalid method parameter for 'a1')
        b = this.a1(new A().a1(2).a4()).a2(); // @error - semantic (invalid right hand side)
        b = this.a1(new A().a2().a1()).a2(); // @error - semantic (invalid member 'a1' on return of 'a2')

        int a;
        int[] ia;
        a = this.a5(b, a, ia, new InvalidNestedMethodCalls()); // @error - semantic (invalid 4th parameter)
        a = this.a5(b, a, ia); // @error - semantic (invalid number of parameters)
        a = this.a5(a, b, ia, new InvalidNestedMethodCalls()); // @error - semantic (invalid order of parameters)
        a = this.a5(b, new A().a3(), ia, new InvalidNestedMethodCalls()); // @error - semantic (invalid 2nd parameter)
        // .. try more of these

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

    public int a5(boolean a, int b, int[] c, A d) {
        return 1;
    }

}
