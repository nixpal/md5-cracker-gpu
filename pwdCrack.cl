

__kernel void findMD5(__global int* pwdArr,
                    __global int* found,
                    __global int *bool2,
                    __global int* md5Arr
                   
                    )

{


int i = get_global_id(0);
int gid = get_group_id(0);
int gsize = get_local_size(0);


if(pwdArr[i] == 0){
    int mvFwd = i-16;
    int c = 0;
    while(md5Arr[c] == pwdArr[mvFwd]){
        c++;
        mvFwd++;
        if (c == 16) break;

    }
    if(c == 16){
        bool2[0] = 1;
        c = 0;
        mvFwd = i-16;
        while(c < 16){
            found[c++] = pwdArr[mvFwd++];
        }

    }

}
    
}








