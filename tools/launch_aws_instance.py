import boto3
import time
import argparse

def launch_instance(region, ami_id, instance_type, key_name, security_group, subnet_id, aws_access_key_id, aws_secret_access_key):
    # Initialize a session using Amazon EC2 with explicit credentials
    session = boto3.Session(
        aws_access_key_id=aws_access_key_id,
        aws_secret_access_key=aws_secret_access_key,
        region_name=region
    )
    ec2_client = session.client('ec2')

    # Launch the FreeBSD 14 instance
    response = ec2_client.run_instances(
        ImageId=ami_id,
        InstanceType=instance_type,
        KeyName=key_name,
        SecurityGroupIds=[security_group],
        SubnetId=subnet_id,
        MinCount=1,
        MaxCount=1
    )

    instance_id = response['Instances'][0]['InstanceId']
    print(f"Launching instance {instance_id}...")

    # Wait for the instance to be in the running state
    print("Waiting for instance to enter running state...")
    while True:
        response = ec2_client.describe_instances(InstanceIds=[instance_id])
        state = response['Reservations'][0]['Instances'][0]['State']['Name']
        if state == 'running':
            break
        print(f"Current state: {state}")
        time.sleep(10)

    # Output instance details
    instance_details = ec2_client.describe_instances(InstanceIds=[instance_id])
    public_ip = instance_details['Reservations'][0]['Instances'][0].get('PublicIpAddress', 'No public IP assigned')
    print(f"Instance {instance_id} is up and running!")
    print(f"Instance ID: {instance_id}")
    print(f"Public IP: {public_ip}")
    print(f"State: {state}")

    # Optional: Tag the instance
    ec2_client.create_tags(
        Resources=[instance_id],
        Tags=[{'Key': 'Name', 'Value': 'FreeBSD14Instance'}]
    )
    print(f"Instance {instance_id} has been tagged with Name=FreeBSD14Instance")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Launch a FreeBSD 14 EC2 instance in AWS.")
    parser.add_argument('--region', required=True, help="AWS region where the instance will be launched.")
    parser.add_argument('--ami-id', required=True, help="AMI ID for FreeBSD 14.")
    parser.add_argument('--instance-type', required=True, help="Instance type, e.g., t3.micro.")
    parser.add_argument('--key-name', required=True, help="Name of the key pair.")
    parser.add_argument('--security-group', required=True, help="ID of the security group.")
    parser.add_argument('--subnet-id', required=True, help="ID of the subnet.")
    parser.add_argument('--aws-access-key-id', required=True, help="AWS Access Key ID.")
    parser.add_argument('--aws-secret-access-key', required=True, help="AWS Secret Access Key.")

    args = parser.parse_args()

    launch_instance(args.region, args.ami_id, args.instance_type, args.key_name, args.security_group, args.subnet_id, args.aws_access_key_id, args.aws_secret_access_key)
