public class BinarySearch{
    public static void main(String[] a){
		System.out.println(new BS().Start(20));
    }
}
// This class contains an array of integers and
// methods to initialize, print and search the array
// using Binary Search

class BS{
    int[] number ;
    int size ;

    // Invoke methods to initialize, print and search
    // for elements on the array
    public int Start(int sz){
		int aux01 ;
		aux01 = this.Init(sz);

		int aux02 ;
		aux02 = this.Print();

		if (this.Search(8)) System.out.println(1) ;
		else System.out.println(0) ;
		if (this.Search(19)) System.out.println(1) ;
		else System.out.println(0) ;
		if (this.Search(20)) System.out.println(1) ;
		else System.out.println(0) ;
		if (this.Search(21)) System.out.println(1) ;
		else System.out.println(0) ;
		if (this.Search(37)) System.out.println(1) ;
		else System.out.println(0) ;
		if (this.Search(38)) System.out.println(1) ;
		else System.out.println(0) ;
		if (this.Search(39)) System.out.println(1) ;
		else System.out.println(0) ;
		if (this.Search(50)) System.out.println(1) ;
		else System.out.println(0) ;

		return 999 ;
    }


    // Search for a specific value (num) using
    // binary search
	public boolean Search(int num){
		boolean bs01 ;
		bs01 = false ;

		int right ;
		right = number.length ;
		right = right - 1 ;

		int left ;
		left = 0 ;

		boolean var_cont ;
		var_cont = true ;

		int medium ;
		int aux01 ;
		aux01 = 0 ;	
		
		while (var_cont){
			medium = left + right ;
			medium = this.Div(medium);
			aux01 = number[medium] ;
			if (num < aux01) right = medium - 1 ;
			else left = medium + 1 ;
			if (this.Compare(aux01,num)) var_cont = false ;
			else var_cont = true ;
			if (right < left) var_cont = false ;
		}

		if (this.Compare(aux01,num)) bs01 = true ;
		else bs01 = false ;
		return bs01 ;
	}

	// This method computes and returns the
	// integer division of a number (num) by 2
	public int Div(int num){
		int count01 ;
		count01 = 0 ;

		int count02 ;
		count02 = 0 ;

		int aux03 ;
		aux03 = num - 1 ;

		while (count02 < aux03) {
			count01 = count01 + 1 ;
			count02 = count02 + 2 ;
		}
		return count01 ;	
	}

    
    // This method compares two integers and
    // returns true if they are equal and false
    // otherwise
    public boolean Compare(int num1 , int num2){
		boolean retval ;
		retval = false ;

		int aux02 ;
		aux02 = num2 + 1 ;

		if (num1 < num2) retval = false ;
		else if (!(num1 < aux02)) retval = false ;
		else retval = true ;

		return retval ;
    }

    // Print the integer array
    public int Print(){
		int j ;
		j = 1 ;

		while (j < (size)) {
			System.out.println(number[j]);
			j = j + 1 ;
		}
		System.out.println(99999);
		return 0 ;
    }
    

    // Initialize the integer array
    public int Init(int sz){
		int j ;
		j = 1 ;

		int k ;
		int aux02 ;
		int aux01 ;

		size = sz ;
		number = new int[sz] ;
		
		k = size + 1 ;
		while (j < (size)) {
			aux01 = 2 * j ;
			aux02 = k - 3 ;
			number[j] = aux01 + aux02 ;
			j = j + 1 ;
			k = k - 1 ;
		}
		return 0 ;	
    }

}
