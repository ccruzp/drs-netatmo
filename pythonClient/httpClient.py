import requests

###########################
# Http Client for the drs #
###########################
class HttpClient:

    __URL = 'http://10.35.0.20:5000/'
    
    def acquire(self, person_id, device_id):
        data = { "person_id": person_id, "device_id": device_id }
        r = requests.post(__URL, data=data)
        return r.status_code


    def release(self, device_id):
        data = { "device_id": device_id }
        r = request.post(__URL, data=data }
        return r.status_code


        
