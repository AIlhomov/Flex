public class InvalidArrayInteger {
    public static void main(String[] a) {
        System.out.println(new A().a2().length);
    }
}

class A {

    public A a1(int num) {
        int[] num_aux;
        num_aux[false] = 2; // @error - semantic (invalid type of array index)

        if (num_aux[this.a2()] < 1) // @error - semantic (invalid type of array index)
            num_aux[0] = 1;
        else
            num_aux = this.a2();

        num_aux[3] = num.length; // @error - semantic (member .length is used incorrectly)
        return this;
    }

    public int[] a2() { // @error - semantic (type mismatch)
        int a;
        a[0] = 2; // @error - semantic (trying to use int as int array)
        return a;
    }
}
