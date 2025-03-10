public class A {
	public static void main(String[] a) {
		System.out.println(new Bar().foo());
	}
}

class Bar {
	public int foo() {
		int aux;
		boolean aux2;
		aux = 1;
		aux2 = true;
		aux = this.foo2(aux);
		aux = this.foo3(2, 5, aux2);
		return aux;
	}

	public int foo2(int p1) {
		if (p1 < 2)
			System.out.println(10);
		else
			System.out.println(0);

		return 1;
	}

	public int foo3(int p1, int p2, boolean p3) {
		if (p3) {
			System.out.println(1);
		} else {
			System.out.println(0);
		}
		System.out.println(p1);
		System.out.println(p2 - p1);
		return 4;
	}
}