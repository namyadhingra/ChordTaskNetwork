network ChordNetwork
{
    parameters:
        int N = default(16);
    submodules:
        coordinator: Coordinator;
        client[N]: ClientNode {
            parameters:
                clientId = index;
        }
}
