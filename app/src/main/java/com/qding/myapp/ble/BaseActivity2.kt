package com.qding.myapp.ble

import android.Manifest
import android.content.pm.PackageManager
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat

/**
 *  Created by yangyeah on 2021/6/15
 *
 *  describe:
 */
open class BaseActivity2 : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestPermission()
    }

  fun requestPermission() {
        try {
            val permissions = arrayOf(Manifest.permission.ACCESS_FINE_LOCATION,
                Manifest.permission.CAMERA,
                Manifest.permission.WRITE_EXTERNAL_STORAGE,
                Manifest.permission.READ_EXTERNAL_STORAGE,
                Manifest.permission.CHANGE_NETWORK_STATE)
            val  ungrantedPermissionList = arrayListOf<String>()
            for( permission in permissions){
                val  granted = ContextCompat.checkSelfPermission(this, permission)
                if(PackageManager.PERMISSION_GRANTED != granted){
                    ungrantedPermissionList.add(permission);
                }
            }
            Log.i("BaseActivity2", "requestPermission: $ungrantedPermissionList")
            if(!ungrantedPermissionList.isEmpty()){
                ActivityCompat.requestPermissions(this, ungrantedPermissionList.toArray(arrayOf()), 1)
            }
        } catch (e:Exception) {
            e.printStackTrace();
        }
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        if (requestCode == 1) {
            if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_DENIED) {
                Log.i("", "onRequestPermissionsResult: grant")
            } else {
                Log.i("", "onRequestPermissionsResult: user denied the permission!")
            }
        }
    }
}